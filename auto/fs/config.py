# Get File System Configuration

import json

class Config:

    _config_file = "fs_config.json"

    def get() -> any:
        ''' Get the file system's configurations from the json file. '''
        with open(Config._config_file, 'r') as f:
            data = json.load(f)
        return data


    def print() -> None:
        ''' Print the file system's configurations. '''

        def print_dict(d: dict, indent: int) -> None:
            for key, value in d.items():
                print(indent*" " + f"{key}")
                if isinstance(value, dict):
                    print_dict(value, indent+4)
                else:
                    print(indent*2*" " + f"{value}")
                    
        print("Using the following configurations:\n" + "—"*16)
        print_dict(Config.get(), 0)
        print("—"*24+"\n")