#!/bin/sh
hsbench_samples="https://01.org/sites/default/files/downloads/hyperscan/hsbench-samples.tar.bz2"

hsbench_c() {
  mkdir -p hsbench-samples
  curl "$hsbench_samples" | tar xj -C hsbench-samples
  chmod +x hsbench-samples/run_bench.sh
  cd hsbench-samples
  ./run_bench.sh $(which hsbench)
  cd ..
}

hsbench_python() {
  taskset 1 python hsbench.py -c hsbench-samples/corpora/alexa200.db -e hsbench-samples/pcre/snort_literals -N
  taskset 1 python hsbench.py -c hsbench-samples/corpora/alexa200.db -e hsbench-samples/pcre/snort_pcres -N
  taskset 1 python hsbench.py -c hsbench-samples/corpora/gutenberg.db -e hsbench-samples/pcre/teakettle_2500
}

main() {
  if ! which hsbench >/dev/null 2>&1; then
    echo 'Please install hsbench and ensure it is in $PATH'
    exit 1
  fi
  echo "--- Performing native benchmark ---"
  hsbench_c
  echo "--- Performing Python benchmark ---"
  hsbench_python
}

main "$@"
