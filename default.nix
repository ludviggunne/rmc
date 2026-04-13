{
  pkgs ? import <nixpkgs> {},
}:

  pkgs.stdenv.mkDerivation {
    name = "rmc";
    src = ./.;

    phases = [
      "unpackPhase"
      "buildPhase"
      "installPhase"
    ];

    buildPhase = ''
      make
    '';

    installPhase = ''
      make install PREFIX=$out
    '';
  }
