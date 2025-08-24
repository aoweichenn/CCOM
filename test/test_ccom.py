import subprocess
import os
import sys


def assert_test(expected, input_str):
    """
    测试编译器对输入表达式的处理是否符合预期结果

    参数:
        expected: 预期的退出码
        input_str: 输入的表达式字符串
    """
    try:
        # 运行编译器生成汇编代码
        result = subprocess.run(
            ["./build/bin/ccom", input_str],
            capture_output=True,
            text=True,
            check=True
        )

        # 将输出写入临时汇编文件
        with open("tmp.s", "w") as f:
            f.write(result.stdout)

        # 使用gcc编译汇编文件
        subprocess.run(
            ["gcc", "-static", "-o", "tmp", "tmp.s"],
            check=True,
            capture_output=True,
            text=True
        )

        # 运行生成的可执行文件
        proc = subprocess.run(
            ["./tmp"],
            capture_output=True,
            text=True
        )

        actual = proc.returncode

        # 验证结果
        if actual == expected:
            print(f"{input_str!r} => {actual}")
        else:
            print(f"{input_str!r} => {expected} expected, but got {actual}")
            # 清理临时文件
            cleanup()
            sys.exit(1)

    except subprocess.CalledProcessError as e:
        print(f"Error processing {input_str!r}: {e.stderr}")
        cleanup()
        sys.exit(1)


def cleanup():
    """清理测试过程中生成的临时文件"""
    for file in ["tmp.s", "tmp"]:
        if os.path.exists(file):
            os.remove(file)


def main():
    # 测试算术表达式
    assert_test(0, "0")
    assert_test(42, "42")
    assert_test(21, "5+20-4")
    assert_test(41, " 12 + 34 - 5 ")
    assert_test(47, "5+6*7")
    assert_test(15, "5*(9-6)")
    assert_test(4, "(3+5)/2")
    assert_test(10, "-10+20")
    assert_test(10, "- -10")
    assert_test(10, "- - +10")

    # 测试相等性判断
    assert_test(0, "0==1")
    assert_test(1, "42==42")
    assert_test(1, "0!=1")
    assert_test(0, "42!=42")

    # 测试小于和小于等于
    assert_test(1, "0<1")
    assert_test(0, "1<1")
    assert_test(0, "2<1")
    assert_test(1, "0<=1")
    assert_test(1, "1<=1")
    assert_test(0, "2<=1")

    # 测试大于和大于等于
    assert_test(1, "1>0")
    assert_test(0, "1>1")
    assert_test(0, "1>2")
    assert_test(1, "1>=0")
    assert_test(1, "1>=1")
    assert_test(0, "1>=2")

    print("OK")
    # 清理临时文件
    cleanup()


if __name__ == "__main__":
    main()
