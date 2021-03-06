sampsim
=======

Population and sampling method simulator.

example
-------

Start by generating a population using the `generate` command:

```
./generate -c default_population.conf test
```

This will create a single file, `test.json`, which can be read by the samplers.  It makes use of the
`default_population.conf` file which is found in the `build` directory, or the `aux` directory of the
source tree.  If you wish to generate two csv files instead of a JSON file (one for households and
one for individuals) then add the `--flat_file` option:

```
./generate --flat_file -c default_population.conf test
```

Now sample the population using one of the `sample` commands:

```
./strip_epi_sample test.json strip
```

This will sample the population created in the previous step of size 1000.  Another file,
`strip.json`, which can also be read by the samplers (should you choose to sample the sample you just
created).  Once again, if you wish to generate two csv files instead of a JSON file then add the
`--flat_file` option:

```
./strip_epi_sample --flat_file test.json strip
```

Additionally, the `generate` command has a batch mode capable of creating multiple populations and
sampling them multiple times using a single command using the `--batch_*` options.  For instance, to
create 10 populations using the default configuration use the `--batch_npop` option:

```
./generate -c default_population.conf --batch_npop 10 multi
```

Population files will automatically be numbered.  To skip writing the population files but instead
sample them 10 times using the `arc_epi` sampler use the `--batch_sampler` option:

```
./generate -c default_population.conf --batch_npop 10 --batch_sampler arc_epi \
           --batch_nsamp 10 multi2
```

This will create a total of 100 samples in the form `"multi2.N.M.json"` where `N` is the population
number (from 01 to 10) and `M` is the sample number (from 01 to 10).  The sampler will use default
parameters.  In order to customise the parameters which the sampler uses they must be placed in
a config file and read in using the `--batch.conf` option:

```
./generate -c default_population.conf --batch_npop 10 --batch_sampler arc_epi --batch_nsamp 10 \
           --batch.conf sample.conf multi2
```

For more information and a list of all possible options use the `--help` option when running any
command.
