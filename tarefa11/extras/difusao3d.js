'use strict';

// Quadros calculados em C; o navegador apenas seleciona e desenha as amostras.
class DiffusionField {
  constructor(buffer) {
    if (buffer.byteLength < 20 || buffer.byteLength % 4) throw new Error('Quadros incompletos');
    const view = new DataView(buffer);
    this.data = new Float32Array(buffer.byteLength / 4);
    for (let i = 0; i < this.data.length; i++) this.data[i] = view.getFloat32(i * 4, true);
    const [size, samples, steps, count, dt] = this.data;
    if (size !== 512 || samples !== 65 || steps !== 2100000 || count !== 201 ||
        Math.abs(dt - 0.2) > 1e-8 || this.data.length !== 5 + count * (1 + samples ** 2) ||
        !this.data.every(value => Number.isFinite(value) && value >= 0) ||
        !this.data.subarray(5).every(value => value <= 0.1)) {
      throw new Error('Formato dos quadros inválido');
    }
    this.size = size;
    this.samples = samples;
    this.count = count;
    this.dt = 0.2;
    this.maxTime = steps;
    this.constant = new Float32Array(samples ** 2);
    this.reset();
  }

  reset(mode = 0) {
    if (![0, 1, 2].includes(mode)) throw new RangeError('Modo inválido');
    this.mode = mode;
    this.constant.fill(mode === 2 ? 1 : 0);
    this.initialPeak = mode === 0 ? this.data[5] : mode === 2 ? 1 : 0;
    this.seek(0);
  }

  seek(index) {
    if (!Number.isInteger(index) || index < 0 || index >= this.count) {
      throw new RangeError('Quadro inválido');
    }
    this.index = index;
    const denominator = (this.count - 1) ** 2;
    this.time = Math.floor((this.maxTime * index * index + denominator / 2) / denominator);
    const offset = 5 + index * (1 + this.samples ** 2);
    this.u = this.mode === 0 ? this.data.subarray(offset + 1, offset + 1 + this.samples ** 2) : this.constant;
    this.peak = this.mode === 0 ? this.data[offset] : this.initialPeak;
  }
}

if (typeof module !== 'undefined') module.exports = {DiffusionField};

if (typeof document !== 'undefined') {
  const panel = document.querySelector('[data-diffusion]');
  if (panel) initDiffusion(panel);
}

async function initDiffusion(panel) {
  const canvas = panel.querySelector('canvas');
  const ctx = canvas.getContext('2d');
  if (!ctx) return; // A figura estática permanece disponível.
  const controls = panel.querySelector('fieldset');
  const mode = panel.querySelector('[data-mode]');
  const play = panel.querySelector('[data-play]');
  const timeline = panel.querySelector('[data-time]');
  const yaw = panel.querySelector('[data-yaw]');
  const pitch = panel.querySelector('[data-pitch]');
  const status = panel.querySelector('[data-status]');
  const metrics = panel.querySelector('[data-metrics]');
  let field;
  try {
    status.textContent = 'Carregando os 201 quadros…';
    const response = await fetch(panel.dataset.frames);
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    field = new DiffusionField(await response.arrayBuffer());
  } catch (error) {
    status.textContent = 'Não foi possível carregar a animação. A figura estática permanece disponível; recarregue a página para tentar novamente.';
    return;
  }
  let running = false, frame = 0, startedAt = null, startIndex = 0, dirty = true;
  const number = (value, digits) => value.toLocaleString('pt-BR', {
    minimumFractionDigits: digits, maximumFractionDigits: digits
  });

  function draw() {
    const width = canvas.clientWidth, height = canvas.clientHeight;
    if (!width || !height) return;
    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    canvas.width = Math.round(width * dpr);
    canvas.height = Math.round(height * dpr);
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    const colors = getComputedStyle(panel);
    ctx.fillStyle = colors.getPropertyValue('--card-bg');
    ctx.fillRect(0, 0, width, height);
    const a = Number(yaw.value) * Math.PI / 180, b = Number(pitch.value) * Math.PI / 180;
    const scale = Math.min(width / 3.6, height / 3.1);
    const max = field.mode === 2 ? 1 : 0.1;
    function project(x, y, z) {
      const horizontal = x * Math.cos(a) - y * Math.sin(a);
      const depth = x * Math.sin(a) + y * Math.cos(a);
      return [width / 2 + horizontal * scale,
        height * 0.64 + (depth * Math.sin(b) - z * Math.cos(b)) * scale,
        depth * Math.cos(b) + z * Math.sin(b)];
    }
    function line(points, color, lineWidth = 1) {
      ctx.beginPath();
      points.forEach((p, i) => i ? ctx.lineTo(p[0], p[1]) : ctx.moveTo(p[0], p[1]));
      ctx.strokeStyle = color;
      ctx.lineWidth = lineWidth;
      ctx.stroke();
    }
    const muted = colors.getPropertyValue('--text-muted');
    for (let k = 0; k <= 4; k++) {
      const p = -1 + k / 2;
      line([project(p, -1, 0), project(p, 1, 0)], colors.getPropertyValue('--border'));
      line([project(-1, p, 0), project(1, p, 0)], colors.getPropertyValue('--border'));
    }

    // ponytail: 64×64 faces ordenadas por profundidade; usar WebGL para malhas de desenho maiores.
    const segments = field.samples - 1, vertices = [], faces = [];
    for (let i = 0; i <= segments; i++) {
      for (let j = 0; j <= segments; j++) {
        const row = Math.round(i * (field.size - 1) / segments);
        const col = Math.round(j * (field.size - 1) / segments);
        const value = field.u[i * field.samples + j] / max;
        vertices.push({point: project(2 * col / (field.size - 1) - 1,
          2 * row / (field.size - 1) - 1, value * 1.25), value});
      }
    }
    for (let i = 0; i < segments; i++) {
      for (let j = 0; j < segments; j++) {
        const c = i * (segments + 1) + j;
        const corners = [vertices[c], vertices[c + 1], vertices[c + segments + 2], vertices[c + segments + 1]];
        faces.push({corners, depth: corners.reduce((sum, v) => sum + v.point[2], 0)});
      }
    }
    faces.sort((left, right) => left.depth - right.depth);
    for (const {corners} of faces) {
      const value = corners.reduce((sum, v) => sum + v.value, 0) / 4;
      ctx.beginPath();
      corners.forEach(({point: p}, i) => i ? ctx.lineTo(p[0], p[1]) : ctx.moveTo(p[0], p[1]));
      ctx.closePath();
      ctx.fillStyle = `hsl(${230 - value * 185} 70% ${36 + value * 18}%)`;
      ctx.fill();
      ctx.strokeStyle = ctx.fillStyle;
      ctx.lineWidth = 0.5;
      ctx.stroke();
    }
    ctx.fillStyle = colors.getPropertyValue('--text');
    ctx.font = '12px system-ui';
    ctx.textAlign = 'center';
    for (const [label, end] of [
      ['x (coluna j)', [1.25, -1, 0]], ['y (linha i)', [-1, 1.25, 0]],
      [`u = ${number(max, 1)}`, [-1, -1, 1.4]]
    ]) {
      const origin = project(-1, -1, 0), p = project(...end);
      line([origin, p], muted, 1.5);
      ctx.fillText(label, Math.max(48, Math.min(width - 48, p[0])), Math.max(16, p[1] - 8));
    }
    const drop = field.initialPeak ? (1 - field.peak / field.initialPeak) * 100 : 0;
    metrics.textContent = `Passo ${number(field.time, 0)}/${number(field.maxTime, 0)} · t = ${number(field.time * field.dt, 1)} · ` +
      `pico ≈ ${number(field.peak, 8)} · queda do pico ≈ ${number(drop, 3)}%`;
    canvas.setAttribute('aria-label', `Superfície de u(x,y). ${metrics.textContent}`);
    panel.querySelector('[data-maximum]').textContent = number(max, 1);
    panel.querySelector('[data-step-label]').textContent = number(field.time, 0);
    timeline.setAttribute('aria-valuetext', `Passo ${number(field.time, 0)} de ${number(field.maxTime, 0)}`);
  }

  function requestDraw() {
    dirty = true;
    if (!frame) frame = requestAnimationFrame(tick);
  }

  function pause(message = 'Pausado.') {
    running = false;
    play.textContent = 'Reproduzir';
    status.textContent = message;
  }

  function tick(timestamp) {
    frame = 0;
    if (running) {
      if (startedAt === null) startedAt = timestamp;
      // 30 quadros/s pelo relógio: mesma duração em telas de 60, 120 ou 144 Hz.
      const index = Math.min(field.count - 1,
        startIndex + Math.floor((timestamp - startedAt) * 30 / 1000));
      if (index !== field.index) { field.seek(index); dirty = true; }
      timeline.value = field.index;
    }
    if (dirty) { draw(); dirty = false; }
    if (field.time === field.maxTime && running) pause(`Concluído: ${number(field.maxTime, 0)} passos.`);
    if (running) frame = requestAnimationFrame(tick);
  }

  play.addEventListener('click', () => {
    if (running) pause();
    else {
      if (field.time === field.maxTime) field.reset(field.mode);
      startIndex = field.index;
      startedAt = null;
      running = true;
      play.textContent = 'Pausar';
      status.textContent = 'Reproduzindo. A velocidade da animação não é tempo de benchmark.';
    }
    requestDraw();
  });
  function reset() {
    pause('Campo reiniciado.');
    field.reset(Number(mode.value));
    timeline.value = 0;
    requestDraw();
  }
  panel.querySelector('[data-reset]').addEventListener('click', reset);
  mode.addEventListener('change', reset);
  timeline.addEventListener('input', () => {
    pause('Passo pronto.');
    field.seek(Number(timeline.value));
    requestDraw();
  });
  [yaw, pitch].forEach(input => input.addEventListener('input', requestDraw));
  let drag = null;
  canvas.addEventListener('pointerdown', event => {
    if (event.button !== 0) return;
    drag = {x: event.clientX, y: event.clientY, yaw: Number(yaw.value), pitch: Number(pitch.value)};
    canvas.setPointerCapture(event.pointerId);
  });
  canvas.addEventListener('pointermove', event => {
    if (!drag) return;
    yaw.value = ((drag.yaw + (event.clientX - drag.x) * 0.5) % 360 + 360) % 360;
    pitch.value = Math.max(20, Math.min(75, drag.pitch - (event.clientY - drag.y) * 0.3));
    requestDraw();
  });
  canvas.addEventListener('lostpointercapture', () => { drag = null; });
  document.addEventListener('visibilitychange', () => {
    if (document.hidden) pause('Pausado ao sair da aba.');
  });
  new ResizeObserver(requestDraw).observe(canvas);
  new MutationObserver(requestDraw).observe(document.documentElement, {attributes: true, attributeFilter: ['data-theme']});
  window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', requestDraw);
  controls.disabled = false;
  panel.querySelector('[data-fallback]').hidden = true;
  canvas.hidden = false;
  status.textContent = 'Pronto. Use Reproduzir ou escolha um passo.';
  requestDraw();
}
