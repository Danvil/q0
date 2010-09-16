set title "frame " . it_count
#fn = "/tmp/samples_normal_" . it_count . ".txt"
fn = "/tmp/samples_annealing_" . it_count . ".txt"
plot fn using 1
pause 0.25
it_count = it_count + 1
if(it_count <= it_max) reread

