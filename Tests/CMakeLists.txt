enable_testing()

add_executable(IvyTestVectorMath IvyTestVectorMath.c)
target_link_libraries(IvyTestVectorMath ${PROJECT_NAME} Unity)

target_compile_options(IvyTestVectorMath PUBLIC
	"$<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:"
    -O3
	">"
)

add_test(IvyTestVectorMathTest IvyTestVectorMath)

add_executable(IvyTestArenaMemoryAllocator IvyTestArenaMemoryAllocator.c)
target_link_libraries(IvyTestArenaMemoryAllocator ${PROJECT_NAME} Unity)

target_compile_options(IvyTestArenaMemoryAllocator PUBLIC
	"$<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:"
    -O3
	">"
)

add_test(IvyTestArenaMemoryAllocatorTest IvyTestArenaMemoryAllocator)

