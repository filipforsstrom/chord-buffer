{
  description = "Development environment";
  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = {
    self,
    flake-utils,
    nixpkgs,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        devShell = pkgs.mkShell {
          packages = with pkgs; [
            gcc-arm-embedded-10
            dfu-util
            openocd
          ];
          buildInputs = [
          ];
          shellHook = ''
            export GCC_PATH=${pkgs.gcc-arm-embedded-10}/bin
          '';
        };
      }
    );
}
