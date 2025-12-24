# Strategic Pivot: Speed to Market (2025-12-18)

**Decision:** Defer implementation of automated tests (Unit/Integration) for the MVP phase.
**Rationale:** Focus on rapid feature delivery and "Speed to Market".
**Constraint:** Code must remain "test-friendly" (Dependency Injection, Interface Abstraction, etc.) to facilitate future test implementation.
**Impact:**
- No `test/` files will be created during this sprint.
- Manual testing (Serial Monitor, Hardware) becomes the primary validation method.
- Architecture patterns supporting testability (DI, Interfaces) MUST be maintained.
