env = Environment()

SConscript('hmetis__parser/SConstruct')

env.Import('hmetis_source')

env.Program(['wrapper_test.cpp', hmetis_source],
        LIBS=['boost_iostreams', 'boost_filesystem'])
