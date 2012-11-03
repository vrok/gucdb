#!/usr/bin/python
from os import system
import os
from collections import defaultdict
import subprocess
import sys

TEMP_PRE_INPUT = '/tmp/pre_input.txt'
TEMP_INPUT = '/tmp/input.txt'
GRANULARITY = 2
REPEATS = 2
MEMORY_LABEL = 'Memory (MB)'

files = [
    'test_data/Wikipedia_titles',
    'test_data/Length_from_150_to_200',
    'test_data/URLs',
    'test_data/Genome',
#    'text.txt',
]

#binaries = ['./binaries/BerkeleyDB', './binaries/SQLite', './binaries/Own']
#binaries = ['./binaries/sqlite', './binaries/mgr_djb']

class Executable:

    def get_libs_list(self):
        raise NotImplementedError()

    def get_name(self):
        raise NotImplementedError()

    def get_binary(self):
        raise NotImplementedError('abstract method')

    def measure_disk_usage(self):
        #raise NotImplementedError('abstract method')
        size = 0L
        for filename in self._db_files:
            size += os.path.getsize(filename)
        return size


class BerkeleyDBExecutable(Executable):

    def __init__(self):
        self._db_files = ['/tmp/main.bdb']

    def get_binary(self):
        return './binaries/BerkeleyDB'

    def get_libs_list(self):
        return ['db_cxx-5.1']

    def get_name(self):
        return 'bdb'


class SQLiteExecutable(Executable):

    def __init__(self):
        self._db_files = ['/tmp/main.sqlite']

    def get_binary(self):
        return './binaries/SQLite'

    def get_libs_list(self):
        return ['sqlite3']

    def get_name(self):
        return 'sqlite'


class OwnExecutable(Executable):
 
    def __init__(self):
        self._db_files = [
            '/tmp/main.leaves',
            '/tmp/main.leaves.map',
            '/tmp/main.nodes',
            '/tmp/main.nodes.map',
            '/tmp/main.slabinfos',
            '/tmp/main.slabinfos.map',
            '/tmp/main.slabs',
            '/tmp/main.slabs.map']

    def get_binary(self):
        return './binaries/Own'

    def get_libs_list(self):
        return ['modules_lib']

    def get_name(self):
        return 'skodb'


executables = [
    BerkeleyDBExecutable(),
    SQLiteExecutable(),
    OwnExecutable(),
]


wanted_convertes = [
    MEMORY_LABEL,
    'User time (seconds)',
    'System time (seconds)',
#    'Average shared text size (kbytes)',
#    'Average unshared data size (kbytes)',
#    'Average stack size (kbytes)',
#    'Average total size (kbytes)',
#    'Maximum resident set size (kbytes)',
#    'Average resident set size (kbytes)',
    'Major (requiring I/O) page faults',
    'Minor (reclaiming a frame) page faults',
#    'Voluntary context switches',
#    'Involuntary context switches',
#    'Swaps',
    'File system inputs',
    'File system outputs',
#    'Socket messages sent',
#    'Socket messages received',
#    'Signals delivered',
]


def make_name(from_what):
    #return ''.join(map(lambda x: '_' if x.isspace() else x.lower(),
    return ''.join(map(lambda x: '_' if x.isspace() else x,
                   filter(lambda x: x.isalnum() or x.isspace(),
                          from_what)))

def run_o(cmd, cwd=None):
    """Run command in shell, return stdout"""

    if cwd:
        popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True, cwd=cwd)
    else:
        popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    popen.wait()
    return '\n'.join(popen.stdout.readlines())

def run_e(cmd):
    """Run command in shell, return stderr"""

    popen = subprocess.Popen(cmd, stderr=subprocess.PIPE, shell=True)
    popen.wait()
    return "\n".join(popen.stderr.readlines())

def clear_indices():
    print "Clearing indices..."
    system('rm -rf /tmp/main*')

def count_lines(filename):
    return int(run_o("""wc -l "%s" | awk '//{print $1}'""" % filename))

def prep_file_exit(filename, lines, output_file=TEMP_INPUT):
    run_o(""" echo exit > %s """ % output_file)

def prep_file_add_sorted(filename, lines, output_file=TEMP_INPUT):
    #run_o(""" head -n %d "%s" | xargs -d '\n' -n 1 -I{} echo write {} {} >> %s """ % \
    run_o(""" head -n %d "%s" | awk '{print "write $0 $0"}' >> %s """ % \
          (lines, filename, output_file))
    run_o(""" echo exit >> %s """ % output_file)

def prep_file_add_random(filename, lines, output_file=TEMP_INPUT):
    #run_o(""" head -n %d "%s" | rl | xargs -d '\n' -n 1 -I{} echo write {} {} >> %s """ % \
    run_o(""" head -n %d "%s" | rl | awk '{print "write $0 $0"}' >> %s """ % \
          (lines, filename, output_file))
    run_o(""" echo exit >> %s """ % output_file)

def prep_file_read_sorted(filename, lines, output_file=TEMP_INPUT):
    run_o(""" head -n %d "%s" | xargs -d '\n' -n 1 -I{} echo read {} >> %s """ % \
          (lines, filename, output_file))
    run_o(""" echo exit >> %s """ % output_file)

def prep_file_add_remove_add(filename, lines, output_file=TEMP_INPUT):
    run_o(""" head -n %d "%s" | rl > /tmp/temp.txt """ % (lines, filename))
    run_o(""" cat /tmp/temp.txt | xargs -d '\n' -n 1 -I{} echo write {} {} >> %s """ % output_file)
    run_o(""" head -n %d /tmp/temp.txt | rl | xargs -d '\n' -n 1 -I{} echo remove {} >> %s """ % (lines / 2, output_file))
    run_o(""" tail -n %d /tmp/temp.txt | rl | xargs -d '\n' -n 1 -I{} echo write {} {} >> %s """ % (lines / 2, output_file))
    run_o(""" echo exit >> %s """ % output_file)

class Test:

    def prepare_whole_test(self, filename, lines, output_file=TEMP_INPUT):
        raise NotImplementedError('This is a base class')

    def prepare_single_test_iteration(self, filename, lines, output_file=TEMP_INPUT):
        raise NotImplementedError('This is a base class')

    def generate_test(self, filename, lines, output_file=TEMP_INPUT):
        raise NotImplementedError('This is a base class')


class ReadSortedTest(Test):

    def prepare_whole_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_add_sorted(filename, lines, output_file)

    def prepare_single_test_iteration(self, filename, lines, output_file=TEMP_INPUT):
        pass

    def generate_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_read_sorted(filename, lines, output_file)


class AddSortedTest(Test):

    def prepare_whole_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_exit(filename, lines, output_file)

    def prepare_single_test_iteration(self, filename, lines, output_file=TEMP_INPUT):
        clear_indices()

    def generate_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_add_sorted(filename, lines, output_file)


class AddRandomTest(Test):

    def prepare_whole_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_exit(filename, lines, output_file)

    def prepare_single_test_iteration(self, filename, lines, output_file=TEMP_INPUT):
        clear_indices()

    def generate_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_add_random(filename, lines, output_file)


class AddRemoveAddTest(Test):

    def prepare_whole_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_exit(filename, lines, output_file)

    def prepare_single_test_iteration(self, filename, lines, output_file=TEMP_INPUT):
        clear_indices()

    def generate_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_add_remove_add(filename, lines, output_file)


class ReadAfterAddRemoveAddTest(Test):

    def prepare_whole_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_add_remove_add(filename, lines, output_file)

    def prepare_single_test_iteration(self, filename, lines, output_file=TEMP_INPUT):
        pass

    def generate_test(self, filename, lines, output_file=TEMP_INPUT):
        return prep_file_read_sorted(filename, lines, output_file)


prepare_functions = [
#    (prep_file_exit, prep_file_add_sorted),
#    (prep_file_exit, prep_file_add_random),
#    (prep_file_add_sorted, prep_file_read_sorted),
#    (prep_
]

tests = [
    ReadSortedTest(),
    AddSortedTest(),
    AddRandomTest(),
    AddRemoveAddTest(),
    ReadAfterAddRemoveAddTest(),
]


# binarka X rodzaj testu X rodzaj danych wej.
collected_data = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: defaultdict(list)))))

def do_test(executable, test_kind, dataset_kind, granularity, preparator):
    #clear_indices()

    binary = executable.get_binary()
    print 'Running test (%s, %s, %s, %s)' % (binary, test_kind, dataset_kind, granularity)

    for i in xrange(REPEATS):
        print i
        preparator()
        run_e('time -v -o time.out %s < %s > /dev/null 2> /dev/null' % (binary, TEMP_INPUT))
        with open('time.out') as f:
            for line in f.readlines():
                title, value = line.rsplit(':', 1)
                title = title.strip()
                collected_data[dataset_kind][test_kind][title][binary][granularity].append(value)
            collected_data[dataset_kind][test_kind][MEMORY_LABEL][binary][granularity].append(float(executable.measure_disk_usage()) / 1024 / 1024)

def process_file(filename, output_dir):
    lines = count_lines(filename)
    for test in tests:

        print 'Doing preparations'
        clear_indices()
        run_o(""" rm %s """ % TEMP_PRE_INPUT)
        test.prepare_whole_test(filename, lines, TEMP_PRE_INPUT)
        for binary in [x.get_binary() for x in executables]:
            run_e('time %s < %s > /dev/null 2> /dev/null' % (binary, TEMP_PRE_INPUT))

        print 'Will run ' + test.__class__.__name__  #prepare_fun.__name__
        for i in xrange(1, GRANULARITY + 1):
            input_lines = i * (lines / GRANULARITY)
            print 'Preparing %s with %d lines' % (filename, input_lines)
            run_o(""" rm %s """ % TEMP_INPUT)
            test.generate_test(filename, input_lines)

            #for binary in binaries:
            for executable in executables:
                #test.prepare_single_test_iteration(filename, lines)
                do_test(executable, test.__class__.__name__, filename, input_lines, lambda: test.prepare_single_test_iteration(filename, lines))

def do_tests(output_dir):
    for filename in files:
        process_file(filename, output_dir)

def get_base_dir():
    return os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

def build_test_executables():
    print 'Building test executables...'
    for executable in executables:
        print '  building %s' % executable.get_name()
        run_o(""" scons -s %s_compare_test """ % executable.get_name(), cwd=get_base_dir())

if __name__ == '__main__':

    if len(sys.argv) < 2:
        print "Usage: ./do.sh output_directory"
        sys.exit(1)


    build_test_executables()

    do_tests(sys.argv[1])


    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    #plt.plot([1,2,3,4])
    #plt.plot([4,3,2,1])
    #plt.ylabel('some numbers')
    ##plt.show()
    #plt.savefig('test.png')

    num = 1

    from math import sqrt

    for dataset_kind, by_dataset_kind in collected_data.iteritems():
        for test_kind, by_test_kind in by_dataset_kind.iteritems():
            for title, by_title in by_test_kind.iteritems():

                if title not in wanted_convertes:
                    print title
                    continue            

                legend_labels = []

                errorbars = []
                nth = lambda n: lambda arr: arr[n]

                for binary, values in by_title.iteritems():
                    
                    x_axis = []
                    y_axis = []

                    points = []

                    for granurality, measures in values.iteritems():
                        fl_measures = map(float, measures)
                        avg = sum(fl_measures) / len(measures)
                        std = sqrt(sum(map(lambda x: pow(x - avg, 2), fl_measures)) / len(measures))
                        #x_axis.append(granurality)
                        #y_axis.append(avg)
                        points.append((granurality, avg, std))
                    
                    points.sort(cmp=lambda a, b: a[0] - b[0])


                    #plt.plot(map(lambda a: a[0], points), map(lambda a: a[1], points), 'o-', label=binary, alpha=0.7)
                    label = os.path.basename(binary)
                    errorbars.append(plt.errorbar(map(nth(0), points), map(nth(1), points), fmt='.-', yerr=map(nth(2), points), label=label, alpha=0.7))
                    legend_labels.append(label)

                image_title = '%s, %s' % (dataset_kind, test_kind)
                plt.title(image_title)
                plt.ylabel(title)
                plt.xlabel('queries')
                #plt.legend(legend_labels, numpoints=1)
                plt.legend(map(nth(0), errorbars), legend_labels, numpoints=1)
                plt.savefig('%s/%s.png' % (sys.argv[1], make_name("%s  %s  %s" % (dataset_kind, test_kind, title))))
                plt.clf()
                plt.cla()
                num += 1

    print collected_data
