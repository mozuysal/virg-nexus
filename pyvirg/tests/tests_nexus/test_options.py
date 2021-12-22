import unittest
from argparse import ArgumentParser

from virg.nexus.options import OptionItem, OptionGroup, Options

class OptionsTest(unittest.TestCase):
    def setUp(self):
        self.opt_items = [
            OptionItem('s'),
            OptionItem('i', 'integer', int, 42, 'magic integer'),
            OptionItem(None, 'test-flag', bool, 'magic flag'),
            OptionItem('r', None, float, 3.1415926, 'magic real number') ]
        self.opt = Options([OptionGroup('simple', self.opt_items)])

    def test_setup(self):
        self.assertEqual(self.opt_items[0].key_name, 's')
        self.assertEqual(self.opt_items[1].key_name, 'integer')
        self.assertEqual(self.opt_items[2].key_name, 'test_flag')
        self.assertEqual(self.opt_items[3].key_name, 'r')

    def test_empty(self):
        values = self.opt.parse_args([])
        self.assertIsNone(values['s'])
        self.assertEqual(values['integer'], 42)
        self.assertFalse(values['test_flag'])
        self.assertEqual(values['r'], 3.1415926)

    def test_full(self):
        values = self.opt.parse_args('--test-flag -i 99 -s abc -r 42.0'.split())
        self.assertEqual(values['s'], 'abc')
        self.assertEqual(values['integer'], 99)
        self.assertTrue(values['test_flag'])
        self.assertEqual(values['r'], 42.0)
