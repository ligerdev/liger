from sys import platform as plf
import os
import shutil
import subprocess
import linecache

def compileOnWindows(qtBin, jomBin, ligerBuild, ligerSource):
    print("Compile on windows.")
    print("Start scripts")
    proc = subprocess.Popen([ligerSource+"\\DevUtils\\WindowsBuild\\buildLigerRelease.bat", qtBin, jomBin, ligerBuild, ligerSource])
    proc.wait()

def compileOnMac(qtBin, ligerBuild, ligerSrc):
    print("Compile on mac.")
    print("Start scripts")
    args = [ligerSource+"/DevUtils/MacOSXBuild/buildLigerReleaseMac.sh", qtBin, ligerBuild, ligerSrc]
    print(args)
    proc = subprocess.Popen(args)
    proc.wait()

def flattenFolderStrucutre(folder):
    print("Flatten " + folder)
    for dirpath, dirnames, filenames in os.walk(folder):
        for filename in filenames:
            shutil.move(os.path.join(dirpath, filename), os.path.join(folder,filename))

def runUnitests(folder, os_plf, ligerSrc, ligerBin):
    print("Run unitests")
    procs=[]
    for dirpath, dirnames, filenames in os.walk(folder):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            if(os_plf == "win32"):
                if(os.path.isfile(filepath) and filepath.endswith('.exe')):
                    exePath=os.path.join(folder, filename)
                    logfilepath = os.path.join(folder, filename.replace(".exe", '_res.txt'))
                    command = [ligerSrc + "\\DevUtils\\RegressionTest\\runOneTest.bat", ligerBin, exePath, logfilepath]
                    print(command)
                    proc = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
                    procs.append(proc)
            elif(os_plf == "darwin"):
                if(os.path.isfile(filepath) and os.access(filepath, os.X_OK) and filepath.find(".dylib") == -1):
                    exePath=os.path.join(folder, filename)
                    logfilepath = os.path.join(folder, filename) + "_res.txt"
                    command = [ligerSrc + "/DevUtils/RegressionTest/runOneTest.sh", folder, exePath , logfilepath]
                    print(command)
                    proc = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
                    procs.append(proc)
            else:
                print("Nope...")
    for p in procs:
        p.wait()
            
def collectTestResults(testFolder):
    print("Collect test results...")
    resultfile = os.path.join(testFolder, "summary_results.html")
    html_str = """
<!DOCTYPE html>
<html>
<head>
<style>
table {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 60%;
}

td, th {
    border: 1px solid #ddd;
    text-align: left;
    padding: 8px;
}

tr:nth-child(even){background-color: #f2f2f2}

tr:hover {background-color: #ddd;}

th {
    padding-top: 12px;
    padding-bottom: 12px;
    background-color: #4CAF50;
    color: white;
}

caption {
    font-weight: bold;
    font-size: 120%;
    caption-side: top;
    padding:10px
}
</style>
</head>
<body>

<table align="center">
     <caption>Table 1 Results for Unitests</caption>
     <tr>
       <th>Name</th>
       <th>Passed</th>
       <th>Failed</th>
       <th>Skipped</th>
       <th>Blacklisted</th>
     </tr>
     <indent>
"""
    for dirpath, dirnames, filenames in os.walk(testFolder):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            if(os.path.isfile(filepath) and filepath.endswith('_res.txt')):
                testfile=os.path.join(testFolder, filename)
                idx = testfile.find("tst_test")
                if(idx != -1):
                    test = testfile[idx:len(testfile)]
                    test = test.replace(".txt", "")
                    print(test)
                else:
                    continue
                count = 0
                fop = open(testfile)
                for line in fop:
                    count += 1
                fop.close()
                res = linecache.getline(testfile,count-1).rstrip()
                if res != "":
                    res = res.replace("Totals: ","").split(",")
                    print(res)
                    html_str += "\t<tr><td><a href=\"" + testfile + "\">"+ test + "</a></td>"
                    for i in range(0, len(res)):
                        html_str += "<td>" + str(res[i].lstrip(" ").split(" ")[0]) + "</td>"
                    html_str += "</tr>\n"
    
    html_str = html_str + """
     </indent>
</table>
</body>
</html>
"""
    output_html= open(resultfile,"w")
    output_html.write(html_str)
    output_html.close()
    print("Done. Open " + resultfile)
    return resultfile
	
if __name__ == "__main__":
    print(plf)
    #mac
    if plf == "darwin":
        qtbin="/Users/yimingyan/Qt5.5.1/5.5/clang_64/bin"
        ligerBuild="/Users/yimingyan/Projects/build_liger_505"
        ligerSource="/Users/yimingyan/Projects/Liger"
        
    #win
    if plf == "win32":
        qtbin="D:\\Qt\\5.5\\msvc2013_64\\bin"
        jombin="D:\\Qt\\Tools\\QtCreator\\bin"
        ligerBuild="E:\\Projects\\build_liger_505"
        ligerSource="E:\\Projects\\Liger"


    print("\t * Qt Bin = " + qtbin)
    print("\t * Liger Build = " + ligerBuild)
    print("\t * Liger Source " + ligerSource)

    ligerBin = os.path.join(ligerBuild, "bin")
    testFolder = os.path.join(ligerBuild, "tests")
    
    if plf == "win32":
        compileOnWindows(qtbin, jombin, ligerBuild, ligerSource)
        flattenFolderStrucutre(testFolder)
        runUnitests(testFolder,plf,ligerSource,ligerBin)
        resultfile = collectTestResults(testFolder)
        os.system("start " + resultfile)
    elif plf == "darwin":
        compileOnMac(qtbin, ligerBuild, ligerSource)
        flattenFolderStrucutre(testFolder)
        runUnitests(testFolder,plf,ligerSource,ligerBin)
        resultfile = collectTestResults(testFolder)
        os.system("open " + resultfile)
    else:
        print("Nope...")
        
