git clone -b loongson git@github.com:wuxn9999/AliOS-Things.git
#安装环境
##用miniconda3装虚拟环境，在虚拟环境中安装aos-tools
conda create --name aos_env
conda activate aos_env
conda install pip
pip install -U aos-tools
##下载编译链
wget http://mirrors.loongnix.cn:8000/toolchain/gcc/release/mips/gcc7/mips-2019.01-15-loongson-sde-elf.tar.bz2
tar -zxjf mips-2019.01-15-loongson-sde-elf.tar.bz2
将/bin添加到环境变量
##编译代码
cd AliOS-Things
aos make helloworld_demo@qemu_loongson1c -c config
aos make
#运行
##download qemu
git clone git@gitee.com:martinqiao/qemu.git
mkdir build
../configure --target-list=mipsel-softmmu,mips64el-softmmu --disable-werror
make
##run
mipsel-softmmu/qemu-system-mipsel -M ls1c -m 32 -serial null -serial null -serial mon:stdio -nic user,id=u1,hostfwd=tcp::4321-:1234 -object filter-dump,id=f1,netdev=u1,file=dump.pcap -nographic -kernel ../../AliOS-Things/solutions/helloworld_demo/out/helloworld_demo@qemu_loongson1c.elf
