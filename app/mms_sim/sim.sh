mkdir -p app/mms_sim
g++ app/mms_sim/main.cc app/mms_sim/sim/mms.cc common/core/algorithm/floodfill.cc -Icommon/core/algorithm -Iapp/mms_sim/sim -o app/mms_sim/sim.exe
