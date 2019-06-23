%pure-parser

%token T_MODE
%token T_WORD
%token T_FUNC
%token T_NUM

%token T_BP T_SP 
%token T_CS T_DS 
%token T_R0 T_R1 
%token T_R2

%%

start: 
    mode prog
  ;

mode: 
    T_MODE num { set_mode($1); }
  | /* empty */
  ;

prog: 
    func
  | prog func
  ;

func: 
    T_FUNC { beg_func($1); } 
    body
  ;

body: 
    inst      { $$ = $1; }
  | body inst { $$ = $1; }
  ;

inst:
    name mod         { inst_rm($1, $2); }
  | name reg ',' mod { inst_r_rm($1, $2, $3); }
  | name mod ',' imm { inst_rm_imm($1, $2, $3); }
  | name sym         { inst_sym($1, $2); }
  ;

mod: 
    reg                 { $$ = make_mod($1, false); }
  | '[' reg ']'         { $$ = make_mod($2, true, 0); }
  | '[' reg '+' off ']' { $$ = make_mod($2, true, $4); }
  | '[' reg '-' off ']' { $$ = make_mod($2, true, $4); }
  ;

reg: 
    T_SP { $$ = $1; }
  | T_BP { $$ = $1; }
  | T_DS { $$ = $1; }
  | T_CS { $$ = $1; }
  | T_R0 { $$ = $1; }
  | T_R1 { $$ = $1; }
  | T_R2 { $$ = $1; }
  ;

name:
    T_WORD = { $$ = $1; }
  ;

sym: 
    T_WORD { loc_sym($1); $$ = $1; }
  ;

off:
    T_NUM { $$ = $1; }
  ;

imm:
    T_NUM { $$ = $1; }
  ;

num:
    T_NUM { $$ = $1; }
  ;

%%
