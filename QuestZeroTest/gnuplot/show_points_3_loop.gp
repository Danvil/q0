fn = "/tmp/samples_" . it_count . ".txt"
splot fn using 2:3:4
pause 0.5
it_count = it_count + 1
if(it_count < it_max) reread

