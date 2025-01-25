{
  pkgs ? import <nixpkgs> {},
}:

pkgs.stdenv.mkDerivation {
  name = "rmc";
  version = "1.0";
  src = ./.;
  buildInputs = with pkgs; [ pkg-config libnotify ];
  buildPhase = ''
    make WITH_LIBNOTIFY=1
  '';
  installPhase = ''
    make install PREFIX=$out
  '';
}
