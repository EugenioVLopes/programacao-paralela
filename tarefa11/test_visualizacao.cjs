// Executar: node tarefa11/test_visualizacao.cjs (requer GCC).
const assert = require('node:assert/strict');
const {execFileSync} = require('node:child_process');
const {mkdtempSync, readFileSync, writeFileSync, rmSync} = require('node:fs');
const {tmpdir} = require('node:os');
const {join} = require('node:path');
const {DiffusionField} = require('../assets/difusao3d.js');

const directory = mkdtempSync(join(tmpdir(), 'difusao3d-test-'));
const compile = (source, binary) => execFileSync('gcc',
  ['-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', source, '-lm', '-o', binary]);
try {
  const published = readFileSync(join(__dirname, 'quadros_difusao.bin'));
  const buffer = published.buffer.slice(published.byteOffset, published.byteOffset + published.byteLength);
  const field = new DiffusionField(buffer);
  assert.equal(field.count, 201);
  assert.equal(field.maxTime, 2100000);
  const initialPeak = field.peak;
  let previous = initialPeak;
  let previousTime = -1;
  for (let index = 0; index < field.count; index++) {
    field.seek(index);
    assert.equal(field.time, Math.round(52.5 * index * index));
    assert.ok(field.time > previousTime);
    previousTime = field.time;
    assert.ok(field.peak <= previous && field.peak > 0);
    previous = field.peak;
    for (let i = 0; i < field.samples; i++) {
      assert.equal(field.u[i], 0);
      assert.equal(field.u[(field.samples - 1) * field.samples + i], 0);
      assert.equal(field.u[i * field.samples], 0);
      assert.equal(field.u[i * field.samples + field.samples - 1], 0);
    }
    assert.ok(field.u.every(value => value <= field.peak));
  }
  assert.equal(field.time, 2100000);
  assert.ok(field.peak / initialPeak > 0.009 && field.peak / initialPeak < 0.01);
  // Modos próprios do mesmo stencil discreto com bordas zero; evita refazer
  // 2,1 milhões de passos. Neste instante, os modos acima de 9 são desprezíveis.
  const modes = [];
  for (let k = 1; k <= 9; k += 2) {
    let coefficient = 0;
    for (let x = 1; x < 511; x++) {
      coefficient += Math.exp(-((x - 255.5) ** 2) / (2 * 51.2 ** 2)) * Math.sin(Math.PI * k * x / 511);
    }
    modes.push({weight: 2 / 511 * coefficient * Math.sin(Math.PI * k * 255 / 511),
      decay: Math.sin(Math.PI * k / 1022) ** 2});
  }
  let expectedFinalPeak = 0;
  for (const a of modes) for (const b of modes) {
    expectedFinalPeak += 0.1 * a.weight * b.weight *
      Math.exp(field.maxTime * Math.log1p(-4 * (0.1 * 0.2) * (a.decay + b.decay)));
  }
  assert.ok(Math.abs(field.peak - expectedFinalPeak) < 4e-11,
    `Pico final ${field.peak}, esperado ${expectedFinalPeak}`);
  const reference = readFileSync(join(__dirname, 'v0_seq.c'), 'utf8');
  for (const index of [1, 20]) {
    field.seek(index);
    const steps = field.time;
    const source = join(directory, `seq-${steps}.c`);
    const binary = join(directory, `seq-${steps}`);
    const output = join(directory, 'campo.txt');
    assert.ok(reference.includes('const int num_passos_tempo = 500;'));
    writeFileSync(source, reference.replace('const int num_passos_tempo = 500;',
      `const int num_passos_tempo = ${steps};`));
    compile(source, binary);
    execFileSync(binary, ['0', output]);
    const expected = readFileSync(output, 'utf8').trim().split(/\s+/).map(Number);
    assert.equal(expected.length, 512 * 512);
    let peak = 0;
    for (const value of expected) peak = Math.max(peak, value);
    assert.equal(field.peak, Math.fround(peak));
    for (let i = 0; i < field.samples; i++) {
      for (let j = 0; j < field.samples; j++) {
        const row = Math.round(i * 511 / 64), col = Math.round(j * 511 / 64);
        assert.equal(field.u[i * field.samples + j], Math.fround(expected[row * 512 + col]));
      }
    }
    // Os modos invariantes também são conferidos na malha completa da referência.
    if (index === 20) for (const mode of [1, 2]) {
      execFileSync(binary, [String(mode), output]);
      const value = mode === 2 ? 1 : 0;
      assert.ok(readFileSync(output, 'utf8').trim().split(/\s+/).every(x => Number(x) === value));
      field.reset(mode);
      field.seek(field.count - 1);
      assert.equal(field.peak, value);
      assert.ok(field.u.every(x => x === value));
    }
  }
  field.reset();
  assert.equal(field.time, 0);
  assert.equal(field.peak, initialPeak);
  assert.throws(() => field.reset(3), RangeError);
  for (const index of [-1, 0.5, 201, NaN]) assert.throws(() => field.seek(index), RangeError);
  assert.throws(() => new DiffusionField(buffer.slice(0, -4)));
  for (const offset of [0, 8, 16, 20, 24]) {
    const invalid = buffer.slice(0);
    new DataView(invalid).setFloat32(offset, NaN, true);
    assert.throws(() => new DiffusionField(invalid));
  }
  console.log('OK: 201 quadros de 0 a 2.100.000 passos; amostras iguais ao C nos passos 53 e 21.000; pico final conferido pelos modos do stencil; bordas, modos invariantes e validação.');
} finally {
  rmSync(directory, {recursive: true, force: true});
}
