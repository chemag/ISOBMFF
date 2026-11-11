---
type: "todo"
title: "isobmff TODO"
description: "Known gaps in the isobmff fork, starting with unit test and fuzzer coverage of the registered boxes."
tags: ["isobmff", "todo", "testing", "fuzzing"]
author: "Chema Gonzalez"
generated_by: "Metamate"
date: "2026-11-10"
---

# 1. Overview

This document tracks known gaps in the isobmff fork.

| Section | Item                                             | Status     |
|---------|--------------------------------------------------|------------|
| 2       | Unit tests and fuzzers for every registered box  | actionable |


# 2. Unit tests and fuzzers for every registered box

Status: actionable, not started.

Every box registered in `src/Parser.cpp` should have a unit test, and a
fuzzer generated from it with `fuzz/converter.py`. That is not the case:
AV01 had neither until av1C support was added (1df0189), and nothing
noticed, because nothing checks.

As of 7412fe3, 47 boxes are registered. 25 have both a unit test and a
fuzzer. The other 22:

| box  | class | unit test | fuzzer | note                                  |
|------|-------|-----------|--------|---------------------------------------|
| pitm | PITM  | no        | no     |                                       |
| iinf | IINF  | no        | no     |                                       |
| urn  | URN   | no        | no     |                                       |
| iref | IREF  | no        | no     |                                       |
| infe | INFE  | no        | no     |                                       |
| irot | IROT  | no        | no     |                                       |
| dimg | DIMG  | no        | no     |                                       |
| thmb | THMB  | no        | no     |                                       |
| cdsc | CDSC  | no        | no     |                                       |
| colr | COLR  | no        | no     |                                       |
| ispe | ISPE  | no        | no     |                                       |
| ipco | IPCO  | no        | no     |                                       |
| frma | FRMA  | no        | no     |                                       |
| schm | SCHM  | no        | no     |                                       |
| elst | ELST  | yes       | no     | has fuzzer::conv markers              |
| iloc | ILOC  | yes       | no     | has fuzzer::conv markers              |
| ipma | IPMA  | yes       | no     | has fuzzer::conv markers              |
| pixi | PIXI  | yes       | no     | has fuzzer::conv markers              |
| hev1 | HEV1  | yes       | no     | has fuzzer::conv markers              |
| avc3 | AVC3  | yes       | no     | has fuzzer::conv markers              |
| ec-3 | EC3   | yes       | no     | test has no fuzzer::conv markers      |
| dec3 | DEC3  | yes       | no     | test has no fuzzer::conv markers      |

Most of the untested boxes are HEIF (iinf, infe, iref, ipco, ispe, irot,
dimg, thmb, cdsc, pitm) and protected sample entries (frma, schm). colr is
shared with video tracks.

Steps:

* the 6 boxes whose tests already carry `fuzzer::conv` markers only need
  `fuzz/converter.py` run on their tests.
* EC3 and DEC3 need markers added to their tests (DEC3's test drives the
  class directly, not through `Parser::CreateBox()`).
* the 14 untested boxes need unit tests, ideally with data taken from real
  files (media/IMG1.HEIC and media/IMG2.HEIC cover most of the HEIF boxes).
* `fuzz/Makefile` lists its fuzzers by hand (`FUZZERS = ...`), and the
  list is stale: It has 18 entries, and misses most of the fuzzers added
  since. Derive it from `fuzz/*_fuzzer.cpp` instead.
* add a check that keeps this from drifting again, e.g. a CTest that
  compares the boxes registered in `src/Parser.cpp` with the boxes the
  unit tests create.
