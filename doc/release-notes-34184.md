Mining IPC
----------

- `Mining.createNewBlock` now has a `cooldown` behavior (enabled by default)
  that waits for IBD to finish and for the tip to catch up. This usually
  prevents a flood of templates during startup, but is not guaranteed. (#34184)
- `Mining.interrupt()` can be used to interrupt `Mining.waitTipChanged` and
  `Mining.createNewBlock`. (#34184)
