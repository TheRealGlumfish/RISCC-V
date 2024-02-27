project('Team41', 'c', version: '0.1.0', 
  default_options : ['c_std=c18', 'warning_level=3'])

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

lex = find_program('flex', required : true)
yacc = find_program('bison', required : true)

lexgen = generator(lex,
  output : '@PLAINNAME@.yy.c',
  arguments : ['-o', '@OUTPUT@', '@INPUT@'])

bisongen = generator(yacc,
output : ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
arguments : ['@INPUT@', '--defines=@OUTPUT1@', '--output=@OUTPUT0@'])


lexfiles = lexgen.process('src/lexer.flex')
bisonfiles = bisongen.process('src/parser.y')

executable('print_tokens', 'src/print_tokens.c', lexfiles, bisonfiles)

