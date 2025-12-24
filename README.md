# TEMPORA

## Introduction

This artefact provides a prototype implementation of the model-checking pipeline based on translating MITL formulae to Generalized Timed Automata (GTA) and performing reachability/liveness analysis. The tool supports checking satisfiability of MITL formula and performing model checking, over pointwise semantics.

The pipeline is divided into two tools:

1. mitl2gta: Translates input mitl formula to a GTA

2. LiveRGTA: Performs reachability/liveness analysis for GTA

Individual READMEs are available in the two sub-directories.

## Setting up & running the docker image

A docker image of the artifacts is available at:
https://figshare.com/articles/software/Tempora/30490967?file=59187428

We provide a docker image with our tools installed, in tempora.tgz

Run the following command to load the docker image:

```
sudo docker load < tempora.tgz
```

The terminal should then display:

```
Loaded image: tempora:latest
```

Run the following command to start the interactive terminal:

```
sudo docker run -it tempora:latest
```

The tool is installed in /home/tempora.
