rm core
ulimit -s unlimited && ulimit -m 500000 && ulimit -v 2500000 && ./ttblazer/bin/ttblazer $@
