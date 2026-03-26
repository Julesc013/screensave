# Public Headers

Purpose: eventual public headers for shared platform contracts.
Belongs here: stable cross-product types, interfaces, and constants once they are proven.
Does not belong here: private helpers or product-specific declarations.
Type: public runtime contract.

Current stage: Series 03 adds only `scr_entry.h`, a deliberately narrow saver-to-host entry seam used to wire the first real `.scr` host target without freezing the wider public runtime API early.
