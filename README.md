### csap_passwordcheck

Rules
- password must be at least 10 bytes long
- password must not contain user name
- password must not contain more than 3 consecutive identical characters
- password must not contain more than 3 consecutive ascending or descending characters
- password must contain letters, digits, and special characters
- *only support plaintext password

Examples

```sql
-- ok
ALTER USER testuser PASSWORD '45wow77!O0';

-- error: too short
ALTER USER testuser PASSWORD 'tooshort!';

-- error: contains user name
ALTER USER testuser PASSWORD 'testuser1!2@';

-- error: contains only letters
ALTER USER testuser PASSWORD 'qpwoeiruty';

-- error: contains only digits
ALTER USER testuser PASSWORD '1029384756';

-- error: contains only specials
ALTER USER testuser PASSWORD '!)@(#*$&%^)';

-- error: missing letters
ALTER USER testuser PASSWORD '10293*$&%^';

-- error: missing digits
ALTER USER testuser PASSWORD 'q)w(e*r&t^';

-- error: missing specials
ALTER USER testuser PASSWORD 'q0w9e8r7t6';

-- error: password is encrypted
ALTER USER testuser PASSWORD 'md507a112732ed9f2087fa90b192d44e358';
```