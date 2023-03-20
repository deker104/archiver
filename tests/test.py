import filecmp
import os
import shutil
import sys
import subprocess
import tempfile


def are_dir_trees_equal(dir1, dir2):
    dirs_cmp = filecmp.dircmp(dir1, dir2)
    if len(dirs_cmp.left_only)>0 or len(dirs_cmp.right_only)>0 or \
            len(dirs_cmp.funny_files)>0:
        return False
    (_, mismatch, errors) =  filecmp.cmpfiles(
        dir1, dir2, dirs_cmp.common_files, shallow=False)
    if len(mismatch)>0 or len(errors)>0:
        return False
    for common_dir in dirs_cmp.common_dirs:
        new_dir1 = os.path.join(dir1, common_dir)
        new_dir2 = os.path.join(dir2, common_dir)
        if not are_dir_trees_equal(new_dir1, new_dir2):
            return False
    return True


class ArchiverTester:
    class TestCaseFailedException(Exception):
        pass

    def __init__(self, archiver_executable, test_data_dir):
        self.archiver_executable = archiver_executable
        self.test_data_dir = test_data_dir

    def get_test_case_data_dir(self, name):
        return os.path.join(self.test_data_dir, name)

    @staticmethod
    def fail_test_case(name, message):
        print("FAIL [{name}] {message}".format(name=name, message=message))
        raise ArchiverTester.TestCaseFailedException()

    @staticmethod
    def succeed_test_case(name):
        print("OK   [{name}]".format(name=name))

    def run_tests(self):
        all_ok = True
        files = os.listdir(self.test_data_dir)
        for name in files:
            if os.path.isdir(self.get_test_case_data_dir(name)):
                try:
                    tester.test_compression_decompression(name)
                except ArchiverTester.TestCaseFailedException:
                    all_ok = False
        return all_ok

    def test_compression_decompression(self, name):
        try:
            test_case_data_dir = self.get_test_case_data_dir(name)
            test_case_archive = self.get_test_case_data_dir(name + ".arc")
            input_files = sorted(os.listdir(test_case_data_dir))

            with tempfile.NamedTemporaryFile() as output_file:
                subprocess.check_call([self.archiver_executable, "-c", output_file.name] + input_files, cwd=test_case_data_dir)

                if not filecmp.cmp(test_case_archive, output_file.name, shallow=False):
                    self.fail_test_case(name, "compressed file differs from expected")

                with tempfile.TemporaryDirectory() as output_dir:
                    subprocess.check_call([self.archiver_executable, "-d", output_file.name], cwd=output_dir)

                    if not are_dir_trees_equal(test_case_data_dir, output_dir):
                        self.fail_test_case(name, "decompressed files differ from expected")

            self.succeed_test_case(name)
        except subprocess.CalledProcessError:
            self.fail_test_case(name, "archiver finished with non-zero exit code")


if __name__ == "__main__":
    tester = ArchiverTester(archiver_executable=sys.argv[1], test_data_dir=sys.argv[2])

    print("Running archiver tests\nExecutable: {executable}\nTest data: {test_data}".format(executable=tester.archiver_executable, test_data=tester.test_data_dir))

    if not tester.run_tests():
        sys.exit(1)
