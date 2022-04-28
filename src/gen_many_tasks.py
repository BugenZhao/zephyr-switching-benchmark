func = 'thread_function_always_yield'
n = int(input())

for i in range(n):
    print(f'struct k_thread thread_{i:02};\nK_THREAD_STACK_DEFINE(stack_{i:02}, 1024);\n')

print('void spawn_many_tasks() {')
for i in range(1, n):
    print(f'    k_thread_create(&thread_{i:02}, stack_{i:02}, 1024, {func}, NULL, NULL, NULL, 1, K_USER, K_NO_WAIT);')
print('}')
