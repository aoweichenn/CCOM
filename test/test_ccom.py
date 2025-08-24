import subprocess
import os
import sys
import tempfile
import unittest
import time
import traceback
from typing import List, Tuple


class GTestStyleResult(unittest.TextTestResult):
    """修正后的gtest风格结果输出类"""

    def __init__(self, stream, descriptions, verbosity):
        super().__init__(stream, descriptions, verbosity)
        self.start_time = 0.0
        self.test_times = {}
        # 禁用默认的"."输出（核心修正点）
        self._showAll = True  # 强制显示详细信息，替代默认的点
        self._verbose = True

    def startTest(self, test):
        super().startTest(test)
        self.start_time = time.time()

    def addSuccess(self, test):
        super().addSuccess(test)
        elapsed = time.time() - self.start_time
        self.test_times[test.id()] = elapsed
        self.stream.write(f"[       OK ] {test.id()} ({elapsed:.6f}s)\n")
        self.stream.flush()

    def addFailure(self, test, err):
        super().addFailure(test, err)
        elapsed = time.time() - self.start_time
        self.test_times[test.id()] = elapsed
        self.stream.write(f"[  FAILED  ] {test.id()} ({elapsed:.6f}s)\n")
        # 正确处理错误信息（修正：使用traceback获取完整错误）
        self.stream.write("Error details:\n")
        self.stream.write(''.join(traceback.format_exception(*err)))
        self.stream.flush()

    def addError(self, test, err):
        super().addError(test, err)
        elapsed = time.time() - self.start_time
        self.test_times[test.id()] = elapsed
        self.stream.write(f"[  ERROR   ] {test.id()} ({elapsed:.6f}s)\n")
        self.stream.write("Error details:\n")
        self.stream.write(''.join(traceback.format_exception(*err)))
        self.stream.flush()

    def printErrors(self):
        # 重写此方法，避免默认的错误汇总输出（与gtest风格冲突）
        pass

    def printSummary(self):
        """修正的测试总结输出"""
        total = self.testsRun
        failed = len(self.failures)
        errors = len(self.errors)
        passed = total - failed - errors

        self.stream.write("\n")
        self.stream.write("-" * 70 + "\n")
        self.stream.write(f"[==========] {total} tests ran. ({sum(self.test_times.values()):.6f}s total)\n")
        self.stream.write(f"[  PASSED  ] {passed} tests.\n")

        if failed > 0:
            self.stream.write(f"[  FAILED  ] {failed} tests:\n")
            for test, _ in self.failures:
                self.stream.write(f"[  FAILED  ]  {test.id()}\n")

        if errors > 0:
            self.stream.write(f"[  ERROR   ] {errors} tests:\n")
            for test, _ in self.errors:
                self.stream.write(f"[  ERROR   ]  {test.id()}\n")

        if failed + errors > 0:
            self.stream.write(f"\n {failed + errors} FAILED TESTS\n")


class GTestStyleRunner(unittest.TextTestRunner):
    """修正后的测试运行器"""
    resultclass = GTestStyleResult

    def run(self, test):
        result = self._makeResult()
        test(result)
        result.printSummary()
        return result


class TestCompiler(unittest.TestCase):
    """编译器测试用例（修正了测试方法生成逻辑）"""

    arithmetic_tests: List[Tuple[str, int, str]] = [
        ("0", 0, "简单数字"),
        ("42", 42, "正整数"),
        ("5+20-4", 21, "加减混合运算"),
        (" 12 + 34 - 5 ", 41, "带空格的运算"),
        ("5+6*7", 47, "乘法优先级"),
        ("5*(9-6)", 15, "括号改变优先级"),
        ("(3+5)/2", 4, "除法运算"),
        ("-10+20", 10, "负数运算"),
        ("- -10", 10, "双重负号"),
        ("- - +10", 10, "多重符号"),
    ]

    comparison_tests: List[Tuple[str, int, str]] = [
        ("0==1", 0, "不等于比较"),
        ("42==42", 1, "等于比较"),
        ("0!=1", 1, "不等于判断"),
        ("42!=42", 0, "等于判断"),
        ("0<1", 1, "小于比较"),
        ("1<1", 0, "小于等于自身"),
        ("2<1", 0, "大于比较反例"),
        ("0<=1", 1, "小于等于比较"),
        ("1<=1", 1, "等于自身的小于等于"),
        ("2<=1", 0, "大于的小于等于反例"),
        ("1>0", 1, "大于比较"),
        ("1>1", 0, "大于等于自身反例"),
        ("1>2", 0, "小于的大于反例"),
        ("1>=0", 1, "大于等于比较"),
        ("1>=1", 1, "等于自身的大于等于"),
        ("1>=2", 0, "小于的大于等于反例"),
    ]

    @classmethod
    def setUpClass(cls):
        cls.compiler_path = "./build/bin/ccom"
        # 修正：确保临时目录在测试失败时也能清理
        cls.temp_dir = tempfile.TemporaryDirectory(prefix="compiler_test_")
        cls.tmp_s = os.path.join(cls.temp_dir.name, "tmp.s")
        cls.tmp_exe = os.path.join(cls.temp_dir.name, "tmp")

    @classmethod
    def tearDownClass(cls):
        try:
            cls.temp_dir.cleanup()
        except Exception as e:
            print(f"清理临时文件失败: {e}", file=sys.stderr)

    def run_test_case(self, input_str: str, expected: int):
        """修正：更健壮的错误处理"""
        try:
            # 1. 运行编译器
            compile_result = subprocess.run(
                [self.compiler_path, input_str],
                capture_output=True,
                text=True,
                timeout=5
            )

            if compile_result.returncode != 0:
                self.fail(
                    f"编译失败 (返回码: {compile_result.returncode})\n"
                    f"标准错误: {compile_result.stderr}\n"
                    f"标准输出: {compile_result.stdout}"
                )

            # 2. 写入汇编文件
            with open(self.tmp_s, "w") as f:
                f.write(compile_result.stdout)

            # 3. 链接
            link_result = subprocess.run(
                ["gcc", "-static", "-o", self.tmp_exe, self.tmp_s],
                capture_output=True,
                text=True,
                timeout=10
            )

            if link_result.returncode != 0:
                self.fail(
                    f"链接失败 (返回码: {link_result.returncode})\n"
                    f"标准错误: {link_result.stderr}"
                )

            # 4. 运行可执行文件
            run_result = subprocess.run(
                [self.tmp_exe],
                capture_output=True,
                text=True,
                timeout=5
            )

            # 5. 验证结果
            self.assertEqual(
                run_result.returncode, expected,
                f"输入: {input_str!r}\n"
                f"预期退出码: {expected}, 实际: {run_result.returncode}\n"
                f"程序输出: {run_result.stdout}\n"
                f"程序错误: {run_result.stderr}"
            )

        except subprocess.TimeoutExpired:
            self.fail(f"测试超时: {input_str!r}")
        except Exception as e:
            self.fail(f"测试异常: {input_str!r}, 错误: {str(e)}")


# 修正：解决动态方法生成的闭包变量捕获问题
def generate_test_methods():
    # 为算术测试生成方法
    for i, (input_str, expected, desc) in enumerate(TestCompiler.arithmetic_tests):
        # 使用lambda绑定当前循环变量（关键修正）
        def make_test_method(input_str, expected):
            def test_method(self):
                self.run_test_case(input_str, expected)
            test_method.__doc__ = desc
            return test_method

        # 处理方法名特殊字符
        safe_name = input_str.replace(' ', '_') \
            .replace('*', 'mul') \
            .replace('/', 'div') \
            .replace('+', 'plus') \
            .replace('-', 'minus')
        method_name = f"test_arithmetic_{i}_{safe_name}"
        setattr(TestCompiler, method_name, make_test_method(input_str, expected))

    # 为比较测试生成方法
    for i, (input_str, expected, desc) in enumerate(TestCompiler.comparison_tests):
        def make_test_method(input_str, expected):
            def test_method(self):
                self.run_test_case(input_str, expected)
            test_method.__doc__ = desc
            return test_method

        safe_name = input_str.replace(' ', '_') \
            .replace('==', 'eq') \
            .replace('!=', 'ne') \
            .replace('<', 'lt') \
            .replace('<=', 'le') \
            .replace('>', 'gt') \
            .replace('>=', 'ge')
        method_name = f"test_comparison_{i}_{safe_name}"
        setattr(TestCompiler, method_name, make_test_method(input_str, expected))


# 生成测试方法
generate_test_methods()


if __name__ == "__main__":
    runner = GTestStyleRunner(verbosity=1)
    suite = unittest.TestLoader().loadTestsFromTestCase(TestCompiler)
    result = runner.run(suite)
    sys.exit(0 if result.wasSuccessful() else 1)
