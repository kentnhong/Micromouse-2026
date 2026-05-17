Push-Location "$PSScriptRoot\..\.."
mkdir -Force "app/mms_sim" | Out-Null
g++ app/mms_sim/main.cc app/mms_sim/sim/mms.cc common/core/algorithm/floodfill.cc -Icommon/core/algorithm -Icommon/core/periph/irsensor -Icommon/drivers/memory -Icommon/drivers/io -Iapp/mms_sim/sim -o app/mms_sim/sim.exe
Pop-Location
