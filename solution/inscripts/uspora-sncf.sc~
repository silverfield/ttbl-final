load tt data/real/tt_sncf.tt
mod tt rmover last
gen tt subtt last -n 1000
gen tt 2td last
gen td 2ug last
pb ug hdeg last hdeg -l 10
pb ug locsep last locsep -set hdeg
or td dijkstra last
or td uspora last -set locsep
orcomp td last conn 1000 dijkstra uspora
quit
