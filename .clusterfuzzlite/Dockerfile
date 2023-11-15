FROM ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-lite:latest AS LITE_BUILDER

FROM gcr.io/oss-fuzz-base/base-builder:v1
RUN apt-get install -y libbsd-dev
COPY . $SRC/app-plugin-boilerplate
COPY ./.clusterfuzzlite/build.sh $SRC/
COPY --from=LITE_BUILDER /opt/ledger-secure-sdk $SRC/app-plugin-boilerplate/BOLOS_SDK
WORKDIR $SRC/app-plugin-boilerplate