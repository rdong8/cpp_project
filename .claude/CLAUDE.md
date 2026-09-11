# CLAUDE.md

See [AGENTS.md](../AGENTS.md) for general agent instructions.

## Comments

Do not use comments as version control, and do not leak your internal thinking/chain of thought. Common examples include leaving comments comparing against the previous version when asked to refactor something:

>Refactor all uses of a separate `std::array` and a size to instead use `std::inplace_vector`.

BAD:

```diff
--- a/XXX
+++ b/XXX
+// Since this is a circular buffer, it is not changed to `std::inplace_vector`
@@ YYY @@
+// This is `std::inplace_vector` instead of a separate array and size
```
