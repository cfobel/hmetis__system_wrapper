env = Environment()

SConscript('hmetis__parser/SConstruct')

env.Import('hmetis_source')

env.Program(['PartitionReader.cpp', hmetis_source],
        LIBS=['boost_iostreams', 'boost_filesystem'])
