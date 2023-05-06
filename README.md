# YOS

## 工作目录

```
.
├── include		// global include files directory
├── kernel		// kernel code files directory
├── Makefile	// project script
├── Doxyfile // rules to generate document
├── docs.sh  // generate document script
├── .gitignore
├── .gdbinit
├── README.md
└── user			// user code files directory
```

## 使用

### 运行操作系统

```shell
make run
```

使用`<C-a> x`退出操作系统

### 调试

```shell
make gdb
```

然后，新建一个终端，运行`gdb`：

```shell
#example
riscv64-linux-gnu-gdb
```

退出`gdb`同时也会退出`qemu`

### 生成代码文档

```shell
make docs
```

生成文档依赖以下工具

```shell
sudo apt install doxygen
sudo apt install cmake graphviz
sudo apt install texlive-latex-base texlive-fonts-recommended texlive-fonts-extra texlive-latex-extra
sudo apt install latex-cjk-all
```

## 技术设计

### 虚拟内存

#### 内核级虚拟页表

暂时参考`xv6`内核空间分配设计：

![image-20230405193732290](https://raw.githubusercontent.com/YEWPO/yewpoblogonlinePic/main/image-20230405193732290.png)

**不同之处**

- `KERNELBASE`：内核代码的起始地址是**`0x80200000`**。
- 使用了`openSBI`

#### 用户级虚拟页表

空间分配图如下：

![image-20230405195454251](https://raw.githubusercontent.com/YEWPO/yewpoblogonlinePic/main/image-20230405195454251.png)

除了`trampoline`和`trapframe`的内容较为固定以外，其余内容每个用户进程私有。

物理内存采用懒分配机制。

##### PTE保留两位标志位在用户级的使用

| 标志位 | 命名 | 含义                                                         |
| ------ | ---- | ------------------------------------------------------------ |
| 8      | `C`  | 写时复制标志位。该标志位为1时，`PTE`所对应的物理页表对该用户进程可读不可写，写时需要复制该物理页表 |
| 9      | `F`  | 最终所有者标志位。该标志位为1时，`PTE`所对应的物理页表为该用户进程拥有，该用户进程对该物理页表有释放权 |

