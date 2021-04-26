# ardufridge

Arduino fridge controller. Created to control old Silesia fridge with faulty thermostat.

Still WIP, very very WIP.

Needs to make compressor decision on say 3 concurrent readings, comparison of freezer/fridge temps, last time compressor was run (count ticks or sthn, avoid writing to EEPROM every dam time) and for how long (we don't want to turn it on and off every few seconds), sumthing called hysteresis?

If it kills yer fridge, not my fault mkay?
If your fridge will gain kind of consciousness will fly to Mars - dont ask me what to do.
