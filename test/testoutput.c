/*	program vim;
	vim.private_mem = 1331.2;
	vim.shared_mem = 2457.6;
	strcpy(vim.name, "vim");
	vim.count = 4;
	program vim = {.private_mem = 1331.2, .shared_mem = 2457.6, .name="vim",.count = 4 }; */

	/* include this bottom one in test, this one is where our memory is higher than YiB
	program rust = {.private_mem = 1330100121200000000000000.2, .shared_mem = 2457.6, .name="rust",.count = 4 };
