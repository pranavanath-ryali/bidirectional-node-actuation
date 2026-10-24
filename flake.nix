{
  description = "ESP32-S3 and ESP32-C3 C Development Environment with ESP-IDF";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    esp-idf.url = "github:mirrexagon/nixpkgs-esp-dev";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, esp-idf, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.permittedInsecurePackages = [
            "python3.13-ecdsa-0.19.1"
          ];
        };
      in
      {
        devShells.default = esp-idf.devShells.${system}.esp-idf-full.overrideAttrs (old: {
          buildInputs = (old.buildInputs or []) ++ [
            pkgs.clang-tools # Provides clangd for high-performance code completion
            pkgs.git
            pkgs.cmake
            pkgs.ninja
          ];

          shellHook = (old.shellHook or "") + ''
            echo "===================================================="
            echo " ESP32-S3 & ESP32-C3 C Development Environment Ready"
            echo "===================================================="
            echo " Switch target using:"
            echo "   idf.py set-target esp32s3"
            echo "   idf.py set-target esp32c3"
            echo "===================================================="
          '';
        });
      }
    );
}
