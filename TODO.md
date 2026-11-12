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

| Section | Item                                             | Status       |
|---------|--------------------------------------------------|--------------|
| 2       | URN does not parse name and location             | actionable   |
| 3       | Field names that do not match the specification  | actionable   |
| 4       | Resolved                                         | for reference|


# 2. URN does not parse name and location

Status: actionable, small.

`URN` (DataEntryUrnBox, "urn ") only parses the FullBox fields. Its `name`
and `location` strings are not kept, and the class has no getters for them,
so a file that references its media through a URN loses that reference.
`test/URN_unittest.cpp` documents the current behavior.


# 3. Field names that do not match the specification

Status: actionable, not started.

STTS called its per-entry `sample_delta` field `sample_offset`
(`GetSampleOffset()`), which is the name of the CTTS field (the composition
offset, PTS - DTS). That made stts read like it held offsets, and it was
fixed in d0985ed (`GetSampleDelta()`). Look for other boxes with the same
kind of problem.

Steps:

* compare each box's getters, setters, members and dump labels with the
  field names in its ISO/IEC 14496-12 (or codec binding) syntax.
* check the semantics too, not just the names: signedness that changes
  with the box version (CTTS's sample_offset is signed in version 1),
  units (media vs movie timescale), and fixed-point values exposed raw
  (e.g. 16.16 or 2.30).
* rename to the specification's names. A rename changes the public API,
  so update liblcvm, the one known user, in the same change as the
  submodule bump that brings it in.


# 4. Resolved

* unit tests and fuzzers for every registered box. When this list was
  started, 22 of the 47 registered boxes lacked a unit test, a fuzzer, or
  both (AV01 had neither until av1C support was added in 1df0189). All 47
  now have both, `fuzz/Makefile` derives its fuzzer list from the unit
  tests instead of keeping a stale list by hand, and the `box_coverage`
  CTest (`test/check_box_coverage.cmake`) fails when a registered box has
  no unit test or fuzzer creating it. Writing the tests found a parser bug:
  infe version 2/3 did not read `item_name`, and required the optional
  `content_encoding`.
