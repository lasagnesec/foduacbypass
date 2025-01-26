This is my custom BOF i have developed while going through CRTO.<br>
It uses 3 manually executed commands from the CNA and a LOLBAS method to bypass AV signature and behavioral detection.
<p></p>
1. Has CNA that can be loaded in Cobalt Strike. Check `help foduacbypass`.<br>
2. It uses 3 stages to bypass behavioral detection.<br>
3. It uses LOLBAS wlrmdr.exe method to execute the implant and fodhelper.exe
<p></p>
regpersist.cna is a persistent method that registers your implant in the following registries: `Run`, `RunOnce`, `RunServices` and `RunServicesOnce`.
<p></p>
Compile using these methods: https://hstechdocs.helpsystems.com/manuals/cobaltstrike/current/userguide/content/topics/beacon-object-files_how-to-develop.htm
