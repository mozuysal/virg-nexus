import sys
from argparse import ArgumentParser
from pathlib import Path

import yaml

class OptionItem:
    def __init__(self, short_name, long_name=None,
                 type_of=str, default_value=None, help_str=None):
        self.short_name = short_name
        self.long_name = long_name
        self.type_of = type_of
        self.default_value = default_value
        self.help_str = help_str

        self.value = default_value
        key_name = long_name if long_name is not None else short_name
        self.key_name = key_name.replace('-', '_')

    def __str__(self):
        return self.key_name + ': ' + \
            (str(self.value) if self.value is not None else 'null')

    def add_as_argument(self, parser):
        names = ['-' + self.short_name if self.short_name else None,
                 '--' + self.long_name if self.long_name else None]
        names = filter(lambda x: x is not None, names)
        if self.type_of is bool:
            parser.add_argument(*names, action='store_true',
                                help=self.help_str)
        else:
            parser.add_argument(*names, type=self.type_of,
                                default=self.default_value,
                                help=self.help_str)

class OptionGroup:
    def __init__(self, title, items=[]):
        self.title = title
        self.items = items

    def __getitem__(self, idx):
        return self.items[idx]

    def __len__(self):
        return len(self.items)

    def __str__(self):
        return '# ' + self.title + '\n' + \
            '\n'.join([str(item) for item in self.items]) + '\n\n'

    def add_as_arguments(self, parser):
        for item in self.items:
            item.add_as_argument(parser)

    def from_dict(self, dict):
        for item in self.items:
            item.value = dict.get(item.key_name, item.default_value)
        return self

    def to_dict(self):
        return { item.key_name: item.value for item in self.items }


class Options:
    def __init__(self, options=[]):
        self.options = options

    def __str__(self):
        return ''.join([str(g) for g in self.options])

    def add_arguments_to(self, parser):
        for group in self.options:
            group.add_as_arguments(parser)

        parser.add_argument('--from-yaml', default=None,
                            help='load options from yaml file'
                            ' ignoring given arguments')

    def parse_args(self, args):
        parser = ArgumentParser()
        self.add_arguments_to(parser)
        res = parser.parse_args(args)

        if res.from_yaml is None:
            dict = vars(res)
        else:
            with open(res.from_yaml) as fin:
                dict = yaml.load(fin, Loader=yaml.Loader)

        for group in self.options:
            group.from_dict(dict)
        return dict

    def to_dict(self):
        dict = {}
        for group in self.options:
            dict.update(group.to_dict())
        return dict

    def from_dict(self, dict):
        for group in self.options:
            group.from_dict(dict)
        return self

    def save_yaml(self, filename):
        with open(filename, 'w') as fout:
            print(str(self), file=fout, end='')

    def load_yaml(self, filename):
        with open(filename) as fin:
            dict = yaml.load(fin, Loader=yaml.Loader)

        self.from_dict(dict)
        return dict
