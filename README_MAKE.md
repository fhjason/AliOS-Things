#安装环境
##安装aos-tools  ->  用miniconda3装虚拟环境（可选），在虚拟环境中安装aos-tools
conda create --name aos_env
conda activate aos_env
**conda install pip**
**pip install -U aos-tools**
##下载代码
git clone [代码库]
##下载编译链
wget http://mirrors.loongnix.cn:8000/toolchain/gcc/release/mips/gcc7/mips-2019.01-15-loongson-sde-elf.tar.bz2
tar -zxjf mips-2019.01-15-loongson-sde-elf.tar.bz2
将/bin添加到环境变量
##编译代码
cd AliOS-Things
aos make
##运行
./run

