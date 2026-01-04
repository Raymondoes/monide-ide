const { ipcRenderer } = require(`electron`);

document.querySelector(`#minimize`).addEventListener(`click`, () => {
  ipcRenderer.send(`btnminimize`);
});

document.querySelector(`#maximize`).addEventListener(`click`, () => {
  ipcRenderer.send(`btnmaximize`);
});

document.querySelector(`#close`).addEventListener(`click`, () => {
  ipcRenderer.send(`btnclose`);
});
