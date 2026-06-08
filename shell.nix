{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    pkg-config
    clang
    clang-tools
    gnumake
    just
    cloc
    gdb
    musl
    musl.dev
    binutils
  ];

  buildInputs = with pkgs; [];

  shellHook = '''';
}
