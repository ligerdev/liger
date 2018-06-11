import os, shutil
import os.path
from shutil import ignore_patterns

def copyDir(src, dst):
    if os.path.exists(dst):
        shutil.rmtree(dst)
    shutil.copytree(source, dst, ignore=ignore_patterns("CMakeLists.txt"))

    
eigen_path = "D:\\Users\\Sheffield University\\Downloads\\eigen-eigen-07105f7124f9\\eigen-eigen-07105f7124f9"

########
lib_eigen_path = "..\\src\\libs\\eigen"
lib_eigen_path = os.path.join(os.getcwd(), lib_eigen_path)

dest   = os.path.join(lib_eigen_path, "src")
source = os.path.join(eigen_path, "Eigen\\src")

print(source + "==>" + dest)
copyDir(source, dest)

dest   = os.path.join(lib_eigen_path, "unsupported\\Eigen")
source = os.path.join(eigen_path, "unsupported\\Eigen")

print(source + "==>" + dest)
copyDir(source, dest)


    
