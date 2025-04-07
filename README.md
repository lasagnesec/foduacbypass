This is a fork from [this repo](https://github.com/ekichirou/foduacbypass), originally created for CRTO and adapted for my CRTL. Credit to their work.
It uses 3 manually executed commands from the CNA and a LOLBAS method to bypass AV signature and behavioral detection.

regpersist.cna is a persistent method that registers your implant in the following registries: `Run`, `RunOnce`, `RunServices` and `RunServicesOnce`.

1. Has CNA that can be loaded in Cobalt Strike. Check `help foduacbypass`.
2. It uses 3 stages to bypass behavioral detection.<br>
3. It uses LOLBAS wlrmdr.exe method to execute the implant and fodhelper.exe

Make with `make`, you know the drill
Then, import the two aggressor scripts and you're good to go

foduacbypass.txt is a powershell base64 for importing into the exam environment
