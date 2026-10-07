# Large external runtime installers

Place the official MATLAB Runtime 9.9 / R2020b Linux x86_64 installer archive here only if you are legally permitted to redistribute it. Runtime archives are intentionally ignored by Git and are not downloaded by APT.

After building the offline bundle, include the archive with:

```bash
make -f Makefile.download include-mcr MCR_ARCHIVE=/path/to/the/MathWorks/runtime.zip
```

The archive will be copied to `offline-bundle/vendor/`. Install the runtime on the offline target using MathWorks' installer and the official license terms. The app expects it at `/usr/local/MATLAB/MATLAB_Runtime/v99` by default; otherwise set `ELYNXSDR_MCR_ROOT` to its installation directory.
