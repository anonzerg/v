import main

lib_path = "../v.so"
args = ['../mol/MOL_3525.xyz']
r, o = main.run(lib_path, args)

print("Return value:", r)
print("Captured output:")
print(o)
