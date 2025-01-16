#include "postgres.h"

#include "commands/user.h"

PG_MODULE_MAGIC;

static check_password_hook_type prev_check_password_hook = NULL;

/*
 * check_password
 *
 * performs checks on an encrypted or unencrypted password
 * ereport's if not acceptable
 *
 * username: name of role being created or changed
 * password: new password (possibly already encrypted)
 * password_type: PASSWORD_TYPE_* code, to indicate if the password is
 *			in plaintext or encrypted form.
 * validuntil_time: password expiration time, as a timestamptz Datum
 * validuntil_null: true if password expiration time is NULL
 */
static void
check_password(const char *username, const char *shadow_pass, PasswordType password_type, Datum validuntil_time, bool validuntil_null)
{
	if (prev_check_password_hook)
		prev_check_password_hook(username, shadow_pass, password_type, validuntil_time, validuntil_null);

	if (password_type != PASSWORD_TYPE_PLAINTEXT)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("only support plaintext password")));
	}
	else
	{
		const char *password = shadow_pass;
		int pwdlen = strlen(password);
		int i;
		bool has_letter = false, has_digit = false, has_special = false;

		/* enforce minimum length */
		if (pwdlen < 10)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("password is too short"),
					 errdetail("password must be at least 10 bytes long")));

		/* check if the password contains the username */
		if (strstr(password, username))
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("password must not contain user name")));

		/* check password complexity and consecutive character rules */
		for (i = 0; i < pwdlen; i++)
		{
			unsigned char ch = (unsigned char)password[i];

			if (isalpha(ch))
				has_letter = true;
			else if (isdigit(ch))
				has_digit = true;
			else
				has_special = true;

			/* check for three consecutive identical characters */
			if (i >= 2 &&
				password[i] == password[i - 1] &&
				password[i] == password[i - 2])
			{
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("password must not contain more than 3 consecutive identical characters")));
			}

			/* check for three consecutive ascending or descending characters */
			if (i >= 2 &&
				((password[i] == password[i - 1] + 1 &&
				  password[i - 1] == password[i - 2] + 1) ||
				 (password[i] == password[i - 1] - 1 &&
				  password[i - 1] == password[i - 2] - 1)))
			{
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("password must not contain more than 3 consecutive ascending or descending characters")));
			}
		}

		/* ensure password contains letters, digits, and special characters */
		if (!has_letter || !has_digit || !has_special)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("password must contain letters, digits, and special characters")));
	}

	/* all checks passed, password is ok */
}

void _PG_init(void)
{
	/* activate password checks when the module is loaded */
	prev_check_password_hook = check_password_hook;
	check_password_hook = check_password;
}
