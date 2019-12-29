.text
.global module_init
.set module_init, 0xdeadbeef
.global module_fini
module_fini:
	ret
.global module_name
	.string "LOVELAIN"
