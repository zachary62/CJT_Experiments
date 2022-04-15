# Calibration: A Simple Trick for Fast Interactive Join Analytics 

## Introduction
Data analytics over normalized databases typically requires computing and materializing expensive joins. Factorized query execution models execution as message passing between relations in the join graph and pushes aggregations through joins to reduce intermediate result sizes. Although this accelerates query execution, it only optimizes a single query. In contrast, analytics is iterative, and offers novel work-sharing opportunities between queries.

This work shows that carefully materializing messages during query execution can accelerate subsequent aggregation queries by >10^5x as compared to factorized execution, and only incurs a constant factor overhead. The key challenge is that messages are sensitive to the message passing ordering (akin to join ordering). To address this challenge, we borrow the concept of calibration in probabilistic graphical models to materialize sufficient messages to support any ordering. We manifest these ideas in the novel Calibrated Junction Hypertree (CJT) data structure, which is fast to build, aggressively re-uses messages to accelerate future queries, and is incrementally maintainable under updates. We further show how CJTs benefit applications such as OLAP, query explanation, streaming data, and data augmentation for ML. 

## Running CJT

We have implemented three versions of CJT: as a custom C++ query engine that uses worst-case optimal join, as a middleware compiler for cloud databases (Redshift), a compiler to Pandas data frame operations.  Please see each directory for details.
