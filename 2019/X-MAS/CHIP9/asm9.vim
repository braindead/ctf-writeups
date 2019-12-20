syntax keyword Keyword org db seek align rd align incbin incimg makelut
syntax keyword Keyword A B C D E H L BC DE HL SP
syntax match Number "[0-9]\+"
syntax match Number "$[0-9a-fA-F]\+"
syntax match Number "%[01]\+"
syntax match Function "[A-Z]\+"
syntax match Label "[a-z_][a-zA-Z_0-9]*:"
syntax match Comment ";.*"
syntax region String start=+"+ end=+"+ skip=+\\"+
