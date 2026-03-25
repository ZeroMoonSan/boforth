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
  ];

  buildInputs = with pkgs; [ man-pages ];

  shellHook = '''';
}
