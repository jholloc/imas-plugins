from collections import namedtuple

HeaderDoc = namedtuple('HeaderDoc', ('name', 'description'))
FunctionDoc = namedtuple('FunctionDoc', ('arguments', 'ret', 'description'))
Argument = namedtuple('Argument', ('name', 'required', 'type', 'description'))


def print_docs(header: HeaderDoc, functions: dict[str, FunctionDoc]):
    header_desc = '\n'.join(header.description)

    print(f'# {header.name} plugin')
    print()
    print(f'{header_desc}')
    print()
    print(f'## Functions')
    print()
    print(f'The {header.name} plugin responds to the following plugin requests:')
    print()
    for function in functions:
        doc = functions[function]
        function_desc = '\n'.join(doc.description)
        print(f'### {function}')
        if doc.arguments:
            print('*Arguments:*')
            print()
            column_headers = ('Name', 'Required', 'Type', 'Description')
            max_lengths = [len(s) + 2 for s in column_headers]
            for arg in doc.arguments:
                max_lengths[0] = max(max_lengths[0], len(arg.name))
                max_lengths[1] = max(max_lengths[1], len(str(arg.required)))
                max_lengths[2] = max(max_lengths[2], len(arg.type))
                max_lengths[3] = max(max_lengths[3], len(arg.description))
            print('| ' + ' | '.join(s + ' ' * (max_lengths[i] - len(s)) for (i, s) in enumerate(column_headers)) + ' |')
            print('|' + '|'.join('-' * (i + 2) for i in max_lengths) + '|')
            for arg in doc.arguments:
                s1 = ' ' * max(max_lengths[0] - len(arg.name), 0)
                s2 = ' ' * max(max_lengths[1] - len(str(arg.required)), 0)
                s3 = ' ' * max(max_lengths[2] - len(arg.type), 0)
                s4 = ' ' * max(max_lengths[3] - len(arg.description), 0)
                print(f'| {arg.name}{s1} | {arg.required}{s2} | {arg.type}{s3} | {arg.description}{s4} |')
            print()
        else:
            print('*Arguments:* None')
            print()
        if doc.ret:
            print(f'*Returns:* {doc.ret}')
            print()
        print(f'{function_desc}')
        print()


def parse_file(file):
    header = HeaderDoc('', '')
    functions: dict[str, FunctionDoc] = {}

    in_header = True
    in_comment = False
    function = None
    in_arguments = False
    in_return = False

    plugin_name = ''
    plugin_desc = []

    args: list[Argument] = []
    ret: str = ''
    desc: list[str] = []

    for line in file:
        line = line.strip()

        if line.startswith('/*'):
            in_comment = True

        if in_comment and line.startswith('*'):
            if line.startswith('*'):
                text = line[1:].strip()
            else:
                text = line.strip()

            if text.startswith('Name:'):
                in_header = True
                plugin_name = text.lstrip('Name:').strip()

            if in_header and text and not text.startswith('Name:') and not line.startswith('*/'):
                plugin_desc.append(text)

            if text.startswith('Function:'):
                function = text.lstrip('Function:').strip()

            if function and text.startswith('Arguments:'):
                in_arguments = True
            elif text == '':
                in_arguments = False
            elif in_arguments:
                tokens = text.split(maxsplit=4)
                name = tokens[0]
                required = 'required' in tokens[1]
                type = tokens[2][:-1]
                description = tokens[4].replace('|', '&#124;')
                args.append(Argument(name, required, type, description))

            if function and text.startswith('Returns:'):
                in_return = True
            elif text == '':
                in_return = False
            elif in_return:
                ret += text.strip()

            if not line.startswith('*/') and function and not in_arguments and not in_return:
                if text and not text.startswith('Function:') and not text.startswith('@'):
                    desc.append(text)

        if in_comment and line.startswith('*/'):
            if in_header:
                header = HeaderDoc(plugin_name, plugin_desc)
            if function:
                functions[function] = FunctionDoc(args, ret, desc)
            args = []
            ret = ''
            desc = []
            in_header = False
            in_comment = False
            in_arguments = False
            in_return = False
            function = None

    print_docs(header, functions)


def main(args):
    if len(args) != 2:
        print(f'usage {args[0]} file_name')
        raise SystemExit(1)

    file_name = args[1]

    with open(file_name) as file:
        parse_file(file)


if __name__ == '__main__':
    import sys
    main(sys.argv)
